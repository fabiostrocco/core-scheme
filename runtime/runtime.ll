@default_plus = global i32 0
@default_minus = global i32 0
@default_times = global i32 0
@default_divide = global i32 0
@default_equals = global i32 0
@default_less = global i32 0
@default_greater = global i32 0
@default_input = global i32 0
@default_output = global i32 0

;These functions have to be provided into a native file
;for example a runtime C file and liked to the
;binary generated after the linkage with llvm
declare i32 @gcc_print(i32)
declare i32 @gcc_input()
declare void @gcc_rti_error(i32, i32)

%struct.data_type = type { i32, i32 }

define void @rti(i64 %packed_data.coerce, i32 %type) {
  %packed_data = alloca %struct.data_type, align 8
  %1 = alloca i32, align 4
  %2 = bitcast %struct.data_type* %packed_data to i64*
  store i64 %packed_data.coerce, i64* %2, align 1
  store i32 %type, i32* %1, align 4
  %3 = getelementptr inbounds %struct.data_type* %packed_data, i32 0, i32 1
  %4 = load i32* %3, align 4
  %5 = load i32* %1, align 4
  %6 = icmp ne i32 %4, %5
  br i1 %6, label %7, label %11

; <label>:7                                       ; preds = %0
  %8 = getelementptr inbounds %struct.data_type* %packed_data, i32 0, i32 1
  %9 = load i32* %8, align 4
  %10 = load i32* %1, align 4
  call void @gcc_rti_error(i32 %9, i32 %10)
  br label %11

; <label>:11                                      ; preds = %7, %0
  ret void
}

define i32 @convert(i64 %packed_data.coerce) {
  %packed_data = alloca %struct.data_type, align 8
  %1 = bitcast %struct.data_type* %packed_data to i64*
  store i64 %packed_data.coerce, i64* %1, align 1
  %2 = getelementptr inbounds %struct.data_type* %packed_data, i32 0, i32 0
  %3 = load i32* %2, align 4
  ret i32 %3
}

define i64 @create(i32 %value, i32 %type) {
  %1 = alloca %struct.data_type, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 %value, i32* %2, align 4
  store i32 %type, i32* %3, align 4
  %4 = load i32* %2, align 4
  %5 = getelementptr inbounds %struct.data_type* %1, i32 0, i32 0
  store i32 %4, i32* %5, align 4
  %6 = load i32* %3, align 4
  %7 = getelementptr inbounds %struct.data_type* %1, i32 0, i32 1
  store i32 %6, i32* %7, align 4
  %8 = bitcast %struct.data_type* %1 to i64*
  %9 = load i64* %8, align 1
  ret i64 %9
}

define i32 @init() {
entry:
  %plus = ptrtoint i32 (i32, i32)* @internal_add to i32
  store i32 %plus, i32 *@default_plus

  %minus = ptrtoint i32 (i32, i32)* @internal_sub to i32
  store i32 %minus, i32 *@default_minus

  %mult = ptrtoint i32 (i32, i32)* @internal_times to i32
  store i32 %mult, i32 *@default_times

  %divs = ptrtoint i32 (i32, i32)* @internal_divide to i32
  store i32 %divs, i32 *@default_divide

  %eq = ptrtoint i32 (i32, i32)* @internal_equals to i32
  store i32 %eq, i32 *@default_equals

  %lt = ptrtoint i32 (i32, i32)* @internal_lt to i32
  store i32 %lt, i32 *@default_less

  %gt = ptrtoint i32 (i32, i32)* @internal_gt to i32
  store i32 %gt, i32 *@default_greater

  %print = ptrtoint i32 (i32)* @gcc_print to i32
  store i32 %print, i32 *@default_output

  %input = ptrtoint i32 ()* @gcc_input to i32
  store i32 %input, i32 *@default_input

  ret i32 0
}

define i32 @internal_add(i32 %a, i32 %b) {
entry:
  %x = add i32 %a, %b
  ret i32 %x
}

define i32 @internal_sub(i32 %a, i32 %b) {
entry:
  %x = sub i32 %a, %b
  ret i32 %x
}

define i32 @internal_times(i32 %a, i32 %b) {
entry:
  %x = mul i32 %a, %b
  ret i32 %x
}

define i32 @internal_divide(i32 %a, i32 %b) {
entry:
  %x = udiv i32 %a, %b
  ret i32 %x
}

define i32 @internal_equals(i32 %a, i32 %b) {
entry:
  %res = icmp eq i32 %a, %b
  br i1 %res, label %else, label %then

then:
  ret i32 0

else:
  ret i32 1
}

define i32 @internal_lt(i32 %a, i32 %b) {
entry:
  %res = icmp slt i32 %a, %b
  br i1 %res, label %else, label %then

then:
  ret i32 0

else:
  ret i32 1
}

define i32 @internal_gt(i32 %a, i32 %b) {
entry:
  %res = icmp sgt i32 %a, %b
  br i1 %res, label %else, label %then

then:
  ret i32 0

else:
  ret i32 1
}
