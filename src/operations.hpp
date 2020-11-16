#if !defined(OPERATIONS_H)
#define OPERATIONS_H

#include "ast.hpp"
#include "types.hpp"

namespace ops {
using ast::Expr;
using ast::Exprs;
using types::Type;

Type *unify_arith(const Type *l, const Type *r, const char *op);
Type *unify_comp(const Type *l, const Type *r, const char *op);
Type *unify_bitwise(const Type *l, const Type *r, const char *op);
Type *unary_verify(const Type *u, const char *op);
Type *btnot_verify(const Type *u, const char *op);
Type *assign_verify(const Type *l, const Type *r, const char *op);

Expr *address_of(Expr *value);
// Expr *derreference(Expr *value); // TODO
Expr *index_access(Expr *value, Expr *index);
Expr *function_call(Expr *value, ast::Exprs *parameters);

}; // namespace ops

#endif /* OPERATIONS_H */
