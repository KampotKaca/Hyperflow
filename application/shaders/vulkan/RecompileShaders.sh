run_script() {
    for shader in *.vert *.frag; do
        [ -e "$shader" ] || continue
        glslc "$shader" -o "../${shader%.*}.spv"
    done
}

# Watch for changes forever
while true; do
    # Wait for any modify, create, delete event in current dir and subdirs
    inotifywait -r -e modify,create,delete,move --quiet .
    # When something happens, run the script
    run_script
done