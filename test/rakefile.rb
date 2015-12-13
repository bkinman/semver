PROJECT_CEEDLING_ROOT = "vendor/ceedling"
load "#{PROJECT_CEEDLING_ROOT}/lib/ceedling.rb"

Ceedling.load_project

task :default => %w[ test:all release ]

task :coverage =>['gcov:all', 'cov_report']
task :cov_report do
REPORT_DIR = Dir.pwd << "/build/artifacts/test"
GCOV_DATA = Dir.pwd << "/build/gcov/out"
TMP_FILE = Dir.pwd << "/build/artifacts/test" << "/cov.tmp"
system("lcov", "-q", "-d", GCOV_DATA, "-c", "-o", TMP_FILE, "-b", ".")
#system("lcov", "-q", "-e", TMP_FILE, Dir.pwd << "/src/*", "-o", TMP_FILE)
system("lcov", "-q", "-l", TMP_FILE)
system("genhtml", TMP_FILE, "-o",  REPORT_DIR << "/html")
end
