@default_plus = global i64 0
@default_minus = global i64 0
@default_times = global i64 0
@default_divide = global i64 0
@default_equals = global i64 0
@default_less = global i64 0
@default_greater = global i64 0
@default_input = global i64 0
@default_output = global i64 0

; These functions have to be provided into a native file
; for example a runtime C file and liked to the
; binary generated after the linkage with llvm

declare void @gcc_print(i32, i32)
declare i32 @gcc_input()

; RTI Functions

declare void @rti(i64, i32)
declare i32 @convert(i64)
declare i64 @create(i32, i32)
declare i32 @typeof(i64)

; Init runtime environment

define i32 @init() {
entry:
  %plus = ptrtoint i64 (i64, i64)* @internal_add to i32
  %packed_plus = call i64 @create(i32 %plus, i32 2)
  store i64 %packed_plus, i64 *@default_plus

  %minus = ptrtoint i64 (i64, i64)* @internal_sub to i32
  %packed_minus = call i64 @create(i32 %minus, i32 2)
  store i64 %packed_minus, i64 *@default_minus

  %times = ptrtoint i64 (i64, i64)* @internal_times to i32
  %packed_times = call i64 @create(i32 %times, i32 2)
  store i64 %packed_times, i64 *@default_times

  %divide = ptrtoint i64 (i64, i64)* @internal_divide to i32
  %packed_divide = call i64 @create(i32 %divide, i32 2)
  store i64 %packed_divide, i64 *@default_divide

  %eq = ptrtoint i64 (i64, i64)* @internal_equals to i32
  %packed_eq = call i64 @create(i32 %eq, i32 2)
  store i64 %packed_eq, i64 *@default_equals

  %lt = ptrtoint i64 (i64, i64)* @internal_lt to i32
  %packed_lt = call i64 @create(i32 %lt, i32 2)
  store i64 %packed_lt, i64 *@default_less

  %gt = ptrtoint i64 (i64, i64)* @internal_gt to i32
  %packed_gt = call i64 @create(i32 %gt, i32 2)
  store i64 %packed_gt, i64 *@default_greater

  %print = ptrtoint i64 (i64)* @internal_output to i32
  %packed_print = call i64 @create(i32 %print, i32 1)
  store i64 %packed_print, i64 *@default_output

  %input = ptrtoint i64 ()* @internal_input to i32
  %packed_input = call i64 @create(i32 %input, i32 0)
  store i64 %packed_input, i64 *@default_input

  ret i32 0
}

; cscheme built-in functions

define i64 @internal_add(i64 %a, i64 %b) {
entry:
  call void @rti(i64 %a, i32 -2)
  call void @rti(i64 %b, i32 -2)
  %a_raw = call i32 @convert(i64 %a)
  %b_raw = call i32 @convert(i64 %b)
  %x = add i32 %a_raw, %b_raw
  %res = call i64 @create(i32 %x, i32 -2)
  ret i64 %res
}

define i64 @internal_sub(i64 %a, i64 %b) {
entry:
  call void @rti(i64 %a, i32 -2)
  call void @rti(i64 %b, i32 -2)
  %a_raw = call i32 @convert(i64 %a)
  %b_raw = call i32 @convert(i64 %b)
  %x = sub i32 %a_raw, %b_raw
  %res = call i64 @create(i32 %x, i32 -2)
  ret i64 %res
}

define i64 @internal_times(i64 %a, i64 %b) {
entry:
   call void @rti(i64 %a, i32 -2)
   call void @rti(i64 %b, i32 -2)
   %a_raw = call i32 @convert(i64 %a)
   %b_raw = call i32 @convert(i64 %b)
   %x = mul i32 %a_raw, %b_raw
   %res = call i64 @create(i32 %x, i32 -2)
   ret i64 %res
}

define i64 @internal_divide(i64 %a, i64 %b) {
entry:
  call void @rti(i64 %a, i32 -2)
  call void @rti(i64 %b, i32 -2)
  %a_raw = call i32 @convert(i64 %a)
  %b_raw = call i32 @convert(i64 %b)
  %x = udiv i32 %a_raw, %b_raw
  %res = call i64 @create(i32 %x, i32 -2)
  ret i64 %res
}

define i64 @internal_equals(i64 %a, i64 %b) {
entry:
  call void @rti(i64 %a, i32 -2)
  call void @rti(i64 %b, i32 -2)
  %a_raw = call i32 @convert(i64 %a)
  %b_raw = call i32 @convert(i64 %b)
  %res = icmp eq i32 %a_raw, %b_raw
  br i1 %res, label %else, label %then

then:
  %res_false = call i64 @create(i32 0, i32 -2)
  ret i64 %res_false

else:
  %res_true = call i64 @create(i32 1, i32 -2)
  ret i64 %res_true
}

define i64 @internal_lt(i64 %a, i64 %b) {
entry:
  call void @rti(i64 %a, i32 -2)
  call void @rti(i64 %b, i32 -2)
  %a_raw = call i32 @convert(i64 %a)
  %b_raw = call i32 @convert(i64 %b)
  %res = icmp slt i32 %a_raw, %b_raw
  br i1 %res, label %else, label %then

then:
  %res_false = call i64 @create(i32 0, i32 -2)
  ret i64 %res_false

else:
  %res_true = call i64 @create(i32 1, i32 -2)
  ret i64 %res_true
}

define i64 @internal_gt(i64 %a, i64 %b) {
entry:
  call void @rti(i64 %a, i32 -2)
  call void @rti(i64 %b, i32 -2)
  %a_raw = call i32 @convert(i64 %a)
  %b_raw = call i32 @convert(i64 %b)
  %res = icmp sgt i32 %a_raw, %b_raw
  br i1 %res, label %else, label %then

then:
  %res_false = call i64 @create(i32 0, i32 -2)
  ret i64 %res_false

else:
  %res_true = call i64 @create(i32 1, i32 -2)
  ret i64 %res_true
}

define i64 @internal_output(i64 %a) {
entry:
   %a_raw = call i32 @convert(i64 %a)
   %a_type = call i32 @typeof(i64 %a)
   call void @gcc_print(i32 %a_raw, i32 %a_type)
   %unit_res = call i64 @create(i32 0, i32 -1)
   ret i64 %unit_res
}

define i64 @internal_input() {
entry:
   %res = call i32 @gcc_input()
   %res_packed = call i64 @create(i32 %res, i32 -2)
   ret i64 %res_packed
}
