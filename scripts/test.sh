echo ""
echo "Running all tests via CTest …"
echo ""
 
ctest --test-dir build --output-on-failure --build-config Release

gcovr --sonarqube coverage.xml --root ..