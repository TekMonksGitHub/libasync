//////////////////////////////////////////////////////////////////////////////
// C++ Compilation Script
// Generic - plugs into IDEs like VSCode etc.
// XForge - (c) TekMonks
//////////////////////////////////////////////////////////////////////////////
// Folder structure
// project
//   |_ cpp/<all your c++ code>
//   |_ hpp/<all your c++ headers>
//   |_ obj/<all your .o files will go here>
//   |_ bin/<final executable>
//   |_ build/build.xf.js
//////////////////////////////////////////////////////////////////////////////
// Syntax: xforge -c -f <path to this script> -o <name of main executable>
//////////////////////////////////////////////////////////////////////////////

const path = require("path");
const workspaceFolder = require("path").resolve(`${__dirname}/../`);
const readdirAsync = require("util").promisify(require("fs").readdir);
const {c_cpp_compile, c_cpp_link} = require(`${CONSTANTS.EXTDIR}/c_cpp_compiler.js`);

const dirs = {
    cpp_inc_dirs: [`${workspaceFolder}/cpp`, `${workspaceFolder}/hpp`],
    o_dirs: {obj: `${workspaceFolder}/obj`, bin:`${workspaceFolder}/bin`, tests:`${workspaceFolder}/tests`},
    cpp_src_dir: `${workspaceFolder}/cpp`
}

const cmds = {
    CC: `clang++ -std=c++2a -c -stdlib=libc++ --debug`,
    ld: `clang++ -std=c++2a`
}

const build_files = {
    cpp_files: [],  // dynamically calculated
    o_files: [],    // dynamically calculated
    out: `${dirs.o_dirs.bin}/${CONSTANTS.OTHER_ARGS[0]}`
};

// build
exports.make = async _ => {
	try {
        // create output directories, if needed
        await CONSTANTS.SHELL.mkdir("-p", Object.values(dirs.o_dirs));

        // calculate .cpp files and .o files
        for (f of (await readdirAsync(dirs.cpp_src_dir)).filter(f => f.endsWith(".cpp"))) {
            build_files.cpp_files.push(`${dirs.cpp_src_dir}/${f}`);
            build_files.o_files.push(`${dirs.o_dirs.obj}/${path.basename(f,".cpp")}.o`);
        }
        
        // parallel compile C++ code, these files have no dependencies on each other
        await c_cpp_compile(build_files.cpp_files, cmds.CC, dirs.cpp_inc_dirs, dirs.o_dirs.obj);
        
        // build the main executable
		await c_cpp_link(cmds.ld, build_files.o_files, build_files.out);

        CONSTANTS.LOGSUCCESS();
	} catch (err) { 
		CONSTANTS.HANDLE_BUILD_ERROR(err); 
    }
}
