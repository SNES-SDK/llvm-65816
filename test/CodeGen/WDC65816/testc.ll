; RUN: clang --target=wdc65816 < %s | FileCheck %s -check-prefix=16

; TODO write some tests

int foo(void) {
    return 1 << 5;
}

int bar(void) {
    return 43 - 1;
}
