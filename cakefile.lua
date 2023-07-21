-- works with https://github.com/fabriciopashaj/cake
-- commit 470050303ddb11a15e52090b99143c6a6a4143d7
-- NOTE: As https://github.com/fabriciopashaj/cake is highly unstable,
-- there may be errors during build

local cc_compiler
if os.getenv("OS") == "windows" then
   if package.config:sub(1, 1) == "/" then
      cc_compiler = "x86_64-w64-mingw32-gcc"
   else
      cc_compiler = "gcc.exe"
   end
else
   cc_compiler = "gcc"
end

os.execute[[mkdir -p  build/obj build/bin]]

return {
   output = "c-ppm-parser.o",
   paths = {
      source = {
         "lib.c",
      },
   },
   compiler = {
      cc = {
         exe = cc_compiler,
         flags = "-O0 -Wall -Wextra -Werror -Wno-unused-function"
      },
   },
   linker = {
      exe = cc_compiler,
      flags = "-c" -- don't link the object file
   },
}
