fn main() {
    protobuf_codegen::Codegen::new()
        .cargo_out_dir("protos")
        .include(".")
        .input("messages.proto")
        .run_from_script();
}