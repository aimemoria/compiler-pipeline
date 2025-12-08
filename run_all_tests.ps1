# Test all compiler features
# Fixed colors for better visibility in PowerShell

$tests = @(
    'test_math.c',
    'test_order_of_operations.c',
    'test_if.c',
    'test_if_else.c',
    'test_nested_if.c',
    'test_loops.c',
    'test_if_while.c',
    'test_for.c',
    'test_do_while.c',
    'test_nested_loops.c',
    'test_arrays.c',
    'test_functions.c',
    'test_security.c',
    'test_comprehensive.c'
)

foreach ($test in $tests) {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor DarkCyan -BackgroundColor Black
    Write-Host "Testing: $test" -ForegroundColor Black -BackgroundColor Yellow
    Write-Host "========================================" -ForegroundColor DarkCyan -BackgroundColor Black
    .\compiler.exe $test
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Black -BackgroundColor Green
Write-Host "ALL TESTS COMPLETED!" -ForegroundColor Black -BackgroundColor Green
Write-Host "========================================" -ForegroundColor Black -BackgroundColor Green
