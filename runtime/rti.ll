; LLVM functions for RTI

%struct.data_type = type { i32, i32 }
declare void @gcc_rti_error(i32, i32)

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

define i32 @typeof(i64 %packed_data.coerce) {
  %packed_data = alloca %struct.data_type, align 8
  %1 = bitcast %struct.data_type* %packed_data to i64*
  store i64 %packed_data.coerce, i64* %1, align 1
  %2 = getelementptr inbounds %struct.data_type* %packed_data, i32 0, i32 1
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
