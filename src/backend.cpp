#include "backend.hpp"
#include <cstdint>
#include <iostream> // TODO DEBUG

#include "ast.hpp"
#include "types.hpp"
#include "util.hpp"

namespace back {
const size_t null_size = 0;
const size_t word_size = 4;
const size_t base_activ_record_size = 1 * word_size;

const char* wasm_type_ptr = "i32";
const char* wasm_type_inte = "i32";
const char* wasm_type_real = "f32";

const char* label_fp = "fp";
const char* label_sp = "sp";
}; // namespace back

//
// Frontend-defined interfaces
//

namespace types {
using namespace back;

size_t PrimType::get_size() const
{
    switch (this->kind) {
        case PrimType::VOID:
            return 0;
            break;
        case PrimType::CHAR:
            return 1;
            break;
        case PrimType::INTEGER:
        case PrimType::REAL:
            return word_size;
            break;
        default:
            abort();
    }
}

size_t Pointer::get_size() const { return word_size; }

size_t Vector::get_size() const
{
    return this->num_elem * this->get_base()->get_size();
}

size_t Function::get_size() const { return null_size; }

} // namespace types

//
// Code generation
//

namespace back {

const char* prim_type_text(const types::PrimType* type)
{
    using types::PrimType;
    assert(type);
    switch (type->kind) {
        case PrimType::CHAR:
            return wasm_type_inte;
        case PrimType::INTEGER:
            return wasm_type_inte;
        case PrimType::REAL:
            return wasm_type_real;
        case PrimType::VOID:
        default:
            std::cerr << "failed getting wasm type text" << std::endl;
            std::cerr << *type << std::endl;
            abort();
    }
}

/// Gets corresponding representation of a type in WebAssembly
const char* type_text(const types::Type* type)
{
    assert(type);
    if (auto prim = dynamic_cast<const types::PrimType*>(type)) {
        return prim_type_text(prim);
    } else if (dynamic_cast<const types::Pointer*>(type)) {
        return wasm_type_ptr;
    }
    return wasm_type_ptr;
    // std::cerr <<"failed getting type text" << std::endl;
    // std::cerr << *type << std::endl;
    // abort();
}

class Emitter {
    std::ostream& out;

    struct LabelCounter : Counter {
        std::string next_label()
        {
            std::string txt("$");
            txt += this->next();
            return txt;
        }
    };

    struct {
        LabelCounter func;
        LabelCounter global;
        LabelCounter local;
        LabelCounter block;
    } labels;

  public:
    Emitter(std::ostream& out) : out(out) {}

    //
    // Emiting Wasm instructions
    //

    void emmit_comment(const std::string& txt) {
        out << ";; " << txt << std::endl;
    }

    void emit_const_ptr(uint64_t value)
    {
        out << "(" << wasm_type_ptr << ".const " << value << ")" << std::endl;
    }

    void emit_const_int(const std::string& tptxt, uint64_t value)
    {
        out << "(" << tptxt << ".const " << value << ")" << std::endl;
    }

    void emit_const_real(const std::string& tptxt, double value)
    {
        out << "(" << tptxt << ".const " << value << ")" << std::endl;
    }

    void emit_get_local(const std::string& label)
    {
        out << "(get_local $" << label << ")" << std::endl;
    }
    void emit_set_local(const std::string& label)
    {
        out << "(set_local $" << label << ")" << std::endl;
    }
    void emit_tee_local(const std::string& label)
    {
        out << "(tee_local $" << label << ")" << std::endl;
    }

    void emit_get_global(const std::string& label)
    {
        out << "(get_global $" << label << ")" << std::endl;
    }
    void emit_set_global(const std::string& label)
    {
        out << "(set_global $" << label << ")" << std::endl;
    }
    void emit_tee_global(const std::string& label)
    {
        out << "(tee_global $" << label << ")" << std::endl;
    }

    void emit_get_local(const std::string& tptxt, const std::string& label)
    {
        out << "(get_local $" << label << ")" << std::endl;
    }

    void emit_drop(size_t num = 1)
    {
        for (size_t i = 0; i < num; ++i) {
            out << "(drop)" << std::endl;
        }
    }

    void emit_load(const std::string& tptxt, bool is_byte = false)
    {
        out << "(" << tptxt << ".load";
        if (is_byte) {
            out << "8_s";
        }
        out << ")" << std::endl;
    }

    void emit_store(const std::string& tptxt, bool is_byte = false)
    {
        out << "(" << tptxt << ".store";
        if (is_byte) {
            out << "8_s";
        }
        out << ")" << std::endl;
    }

    void emit_call(const std::string& label)
    {
        out << "(call $" << label << ")" << std::endl;
    }

    void emit_get_fp() { emit_get_global(label_fp); }
    void emit_get_sp() { emit_get_global(label_sp); }

    void emit_set_fp() { emit_set_global(label_fp); }
    void emit_set_sp() { emit_set_global(label_sp); }

    void emit_add(const std::string& tptxt)
    {
        out << "(" << tptxt << "."
            << "add"
            << ")" << std::endl;
    }

    //
    // AST handling
    //

    void emit_program(ast::Program* root)
    {
        for (auto child : root->get_children()) {
            if (auto func_def = dynamic_cast<ast::FuncDef*>(child)) {
                labels.local.reset();
                labels.block.reset();

                auto name = func_def->ref.get().name;
                out << "(func ";
                out << "$" << name << std::endl;
                auto block = func_def->get_child();
                emit_stmt(block);
                out << ")" << std::endl;
            }
        }
    }

    void emit_stmt(ast::Statement* stmt)
    {
        if (auto block = dynamic_cast<ast::Block*>(stmt)) {
            for (auto child_stmt : block->get_children()) {
                emit_stmt(child_stmt);
            }
        } else if (auto expr_stmt = dynamic_cast<ast::ExpressionStmt*>(stmt)) {
            out << ";; ";
            stmt->write_repr(out);
            out << std::endl;
            auto expr = expr_stmt->get_child();
            emit_expr(expr);
            auto type = expr->get_type();
            auto size = type->get_size();
            if (size > 0) {
                emit_drop();
                // emit_drop(div_ceil(size, word_size));
            }
        }
    }

    void emit_expr(ast::Expr* expr)
    {
        if (auto var = dynamic_cast<ast::Variable*>(expr)) {
            emit_var(var);
        } else if (auto int_val = dynamic_cast<ast::IntegerValue*>(expr)) {
            emit_int_val(int_val);
        } else if (auto real_val = dynamic_cast<ast::RealValue*>(expr)) {
            emit_real_val(real_val);
        } else if (auto assign_stmt = dynamic_cast<ast::Assign*>(expr)) {
            emit_assign(assign_stmt);
        } else if (auto binop = dynamic_cast<ast::BinOp*>(expr)) {
            emit_simple_binop(binop);
        } else if (auto func_call = dynamic_cast<ast::Call*>(expr)) {
            emit_func_call(func_call);
        } else if (auto derref = dynamic_cast<ast::Derreference*>(expr)) {
            emit_derref(derref);
        }
        // else if (auto derref = dynamic_cast<ast::IndexAccess*>(expr)) {
        // TODO
        // }
        else {
            std::cerr << "NOT IMPLEMENTED: " << (*expr) << std::endl;
            assert(0);
        }
    }

    void emit_var_loc(ast::Variable* var)
    {
        assert(var);
        auto& var_row = var->ref.get();
        auto offset = assert_derref(var_row.offset);

        emit_const_int(wasm_type_ptr, offset);
        // If it is a local variable, sum frame pointer
        if (!var_row.is_global) {
            emit_get_fp();
            emit_add(wasm_type_ptr);
        }
    }

    void emit_var(ast::Variable* var)
    {
        assert(var);
        emit_var_loc(var);

        auto& var_row = var->ref.get();
        auto type = var_row.type;
        auto type_txt = type_text(type);
        bool is_byte = type->is_compatible_with(types::prim_char);
        emit_load(type_txt, is_byte);
    }

    void emit_int_val(const ast::IntegerValue* val_node)
    {
        auto val = val_node->get_value();
        emit_const_int(wasm_type_ptr, val);
    }

    void emit_real_val(const ast::RealValue* val_node)
    {
        auto val = val_node->get_value();
        emit_const_real(wasm_type_real, val);
    }

    void emit_assign(ast::Assign* assign_stmt)
    {
        // auto label = this->labels.local.next_label();  // TODO

        auto target = assign_stmt->get_left();

        auto type = target->get_type();
        auto type_txt = type_text(type);
        char is_byte = type->is_compatible_with(types::prim_char);
        // TODO refactor into emit_store etc

        auto ltarget = assert_ret(dynamic_cast<ast::LExpr*>(target));
        emit_lexpr_loc(ltarget);

        auto source = assign_stmt->get_right();
        emit_expr(source);

        emit_store(type_txt, is_byte);

        emit_expr(source); // TODO use variable
    }

    void emit_func_call(ast::Call* func_call)
    {
        auto func_val = func_call->get_left();
        assert(func_val);
        auto args = func_call->get_right()->get_children();
        for (auto& arg : args) {
            emit_expr(arg);
        }
        if (auto var_node = dynamic_cast<ast::Variable*>(func_val)) {
            auto& var_row = var_node->ref.get();
            auto& var_name = var_row.name;
            // std::cerr << "calling " << var_name << std::endl;  // DEBUG
            emit_call(var_name);
        } else {
            std::cerr << "called value is not a name/symbol " << (*func_val)
                      << std::endl;
        }
    }

    static const char* binop_instr(ast::BinOp* binop)
    {
        assert(binop);
        if (dynamic_cast<ast::Plus*>(binop)) {
            return "add";
        }
        std::cerr << "unknown bin operator " << (*binop) << std::endl;
        abort();
    }

    void emit_simple_binop(ast::BinOp* expr)
    {
        assert(expr);
        emit_expr(expr->get_left());
        emit_expr(expr->get_right());
        auto type = expr->get_type();
        auto type_txt = type_text(type);
        auto instr = binop_instr(expr);
        out << "(" << type_txt << "." << instr << ")" << std::endl;
    }

    void emit_derref(ast::Derreference* derref)
    {
        auto ptr_expr = derref->get_child();
        auto ptype =
            assert_ret(dynamic_cast<types::Pointer*>(ptr_expr->get_type()));
        auto base_type = ptype->get_base();
        auto type_txt = type_text(base_type);
        emit_expr(ptr_expr);
        emit_load(type_txt);
    }

    /**
     * Emits code to compute location of a lexpr on memory
     */
    void emit_lexpr_loc(ast::LExpr* lexpr)
    {
        // action_if<ast::Variable, ast::LExpr>(
        //     lexpr, [this](ast::Variable* e) { emit_var_loc(e); });
        if (auto var = dynamic_cast<ast::Variable*>(lexpr)) {
            emit_var_loc(var);
        } else if (auto derref = dynamic_cast<ast::Derreference*>(lexpr)) {
            auto ptr_expr = derref->get_child();
            emit_expr(ptr_expr);
        }
        // else if (auto access = dynamic_cast<ast::IndexAccess*>(lexpr)) {
        //     auto ptr_expr = access->get_left();
        //     auto idx_expr = access->get_right();
        //     // TODO calc offset
        // }
        else {
            std::cerr << "NOT IMPLEMENTED: " << (*lexpr) << std::endl;
            assert(0);
        }
    }
};

const char* header = R"BLOCK(
(module
    (import "std" "println" (func $println (param i32)))
    (import "std" "println_int" (func $println_int (param i32)))
    (import "std" "println_real" (func $println_real (param f32)))

    (export "memory" (memory $mem))
    (export "str_len" (func $str_len))
    (export "str_copy" (func $str_copy))
    (export "str_end" (func $str_end))
    (export "str_cat" (func $str_cat))
    (export "main" (func $main))

    (memory $mem 1)

    (func $str_len (param $po i32) (result i32)
        (local $idx i32)
        (block $out
            (loop $loop
                local.get $po
                local.get $idx
                i32.add
                i32.load8_u
                i32.eqz
                br_if $out
                local.get $idx
                i32.const 1
                i32.add
                local.set $idx
                br $loop
            )
        )
        local.get $idx
    )
    (func $str_copy (param $dest i32) (param $src i32)
        (local $c i32)
        (block $out
            (loop $loop
                local.get $src
                i32.load8_u
                local.tee $c
                i32.eqz
                br_if $out
                local.get $dest
                local.get $c
                i32.store8
                local.get $src
                i32.const 1
                i32.add
                local.set $src
                local.get $dest
                i32.const 1
                i32.add
                local.set $dest
                br $loop
            )
        )
    )
    (func $str_end (param $po i32) (result i32)
        (block $out
            (loop $loop
                local.get $po
                i32.load8_u
                i32.eqz
                br_if $out
                local.get $po
                i32.const 1
                i32.add
                local.set $po
                br $loop
            )
        )
        local.get $po
    )
    (func $str_cat (param $dest i32) (param $src i32)
        local.get $dest
        call $str_end
        local.get $src
        call $str_copy
    )

)BLOCK";

const char* footer = "\n)\n";

void generate_code(std::ostream& out, ast::Program* root)
{
    out << header;
    // TODO initial stack pointer value
    out << "(global $" << label_fp << " i32 (i32.const 0))" << std::endl;
    out << "(global $" << label_sp << " i32 (i32.const 0))" << std::endl;
    out << std::endl;

    Emitter emt(out);
    emt.emit_program(root);
    out << footer;
}

} // namespace back