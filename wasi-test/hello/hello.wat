(module
  (type (;0;) (func (param i32 i32 i32 i32) (result i32)))
  (type (;1;) (func))
  (type (;2;) (func (param i32) (result i32)))
  (type (;3;) (func (param i32)))
  (import "wasi_unstable" "fd_write" (func (;0;) (type 0)))
  (func (;1;) (type 1))
  (func (;2;) (type 2) (param i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
    global.get 0
    local.set 1
    i32.const 16
    local.set 2
    local.get 1
    local.get 2
    i32.sub
    local.set 3
    local.get 3
    local.get 0
    i32.store offset=12
    local.get 3
    i32.load offset=12
    local.set 4
    local.get 3
    local.get 4
    i32.store offset=8
    block  ;; label = @1
      loop  ;; label = @2
        i32.const 0
        local.set 5
        local.get 3
        i32.load offset=8
        local.set 6
        local.get 6
        i32.load8_u
        local.set 7
        i32.const 255
        local.set 8
        local.get 7
        local.get 8
        i32.and
        local.set 9
        i32.const 255
        local.set 10
        local.get 5
        local.get 10
        i32.and
        local.set 11
        local.get 9
        local.get 11
        i32.ne
        local.set 12
        i32.const 1
        local.set 13
        local.get 12
        local.get 13
        i32.and
        local.set 14
        local.get 14
        i32.eqz
        br_if 1 (;@1;)
        local.get 3
        i32.load offset=8
        local.set 15
        i32.const 1
        local.set 16
        local.get 15
        local.get 16
        i32.add
        local.set 17
        local.get 3
        local.get 17
        i32.store offset=8
        br 0 (;@2;)
      end
    end
    local.get 3
    i32.load offset=8
    local.set 18
    local.get 3
    i32.load offset=12
    local.set 19
    local.get 18
    local.get 19
    i32.sub
    local.set 20
    local.get 20
    return)
  (func (;3;) (type 3) (param i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
    global.get 0
    local.set 1
    i32.const 32
    local.set 2
    local.get 1
    local.get 2
    i32.sub
    local.set 3
    local.get 3
    global.set 0
    i32.const 1
    local.set 4
    i32.const 8
    local.set 5
    local.get 3
    local.get 5
    i32.add
    local.set 6
    local.get 6
    local.set 7
    i32.const 20
    local.set 8
    local.get 3
    local.get 8
    i32.add
    local.set 9
    local.get 9
    local.set 10
    local.get 3
    local.get 0
    i32.store offset=28
    local.get 3
    local.get 4
    i32.store offset=24
    local.get 3
    i32.load offset=28
    local.set 11
    local.get 3
    local.get 11
    i32.store offset=8
    local.get 3
    i32.load offset=28
    local.set 12
    local.get 12
    call 2
    local.set 13
    local.get 3
    local.get 13
    i32.store offset=12
    local.get 4
    local.get 7
    local.get 4
    local.get 10
    call 0
    local.set 14
    local.get 3
    local.get 14
    i32.store16 offset=18
    i32.const 32
    local.set 15
    local.get 3
    local.get 15
    i32.add
    local.set 16
    local.get 16
    global.set 0
    return)
  (func (;4;) (type 1)
    (local i32)
    i32.const 1024
    local.set 0
    local.get 0
    call 3
    return)
  (table (;0;) 1 1 funcref)
  (memory (;0;) 2)
  (global (;0;) (mut i32) (i32.const 66576))
  (global (;1;) i32 (i32.const 1024))
  (global (;2;) i32 (i32.const 1037))
  (global (;3;) i32 (i32.const 1024))
  (global (;4;) i32 (i32.const 66576))
  (global (;5;) i32 (i32.const 0))
  (global (;6;) i32 (i32.const 1))
  (export "memory" (memory 0))
  (export "__wasm_call_ctors" (func 1))
  (export "slen" (func 2))
  (export "print" (func 3))
  (export "_start" (func 4))
  (export "__dso_handle" (global 1))
  (export "__data_end" (global 2))
  (export "__global_base" (global 3))
  (export "__heap_base" (global 4))
  (export "__memory_base" (global 5))
  (export "__table_base" (global 6))
  (data (;0;) (i32.const 1024) "Hello world\0a\00"))
