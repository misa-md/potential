<a name="unreleased"></a>
## [Unreleased]


<a name="v0.4.0"></a>
## [v0.4.0] - 2023-07-26
### Build
- **cmake:** correct cmake project name to 'potential'
- **pkg:** bump googletest to from v1.10.0 to v1.12.0
- **pkg:** bump google/benchmark to v1.8.2 to fix werror error in clang

### Chore
- mark class ArrayMap, InterpolationLists, OneWayEamList and EamPhiList as deprecated
- **parser:** translate comments: use english and remove chinese in comments

### Docs
- **changelog:** add changelog for v0.4.0

### Feat
- **eam:** make eam computation apis as const
- **eam:** add api `eam::embedEnergy` without `max_rho` parameter: auto-detect max_rho from table
- **interpolation:** support spline calculation and potential table from a non-zero point as start
- **parser:** change to use C++ stream to parse eam potential file
- **parser:** add member function `SetflParser::getHeader` for getting header of eam potential file

### Fix
- **compile:** fix building warnings
- **parser:** fix incorrect element filtering in eam/fs parsing when element type filter is enabled

### Merge
- Merge pull request [#14](https://github.com/MISA-MD/potential/issues/14) from misa-md/dev
- Merge pull request [#11](https://github.com/MISA-MD/potential/issues/11) from misa-md/better-code-style-and-warning-fixing
- **eam:** Merge pull request [#9](https://github.com/MISA-MD/potential/issues/9) from misa-md/test-eam-fs-potential-apis
- **eam:** Merge pull request [#7](https://github.com/MISA-MD/potential/issues/7) from misa-md/feature-embed-energy-rhomax-api-redesign
- **eam:** Merge pull request [#6](https://github.com/MISA-MD/potential/issues/6) from misa-md/test-eam-alloy-potential-apis
- **parser:** Merge pull request [#13](https://github.com/MISA-MD/potential/issues/13) from misa-md/test-parser-api-setFilterEleTypes
- **parser:** Merge pull request [#12](https://github.com/MISA-MD/potential/issues/12) from misa-md/feature-potential-parser-api-iostream
- **parser:** Merge pull request [#8](https://github.com/MISA-MD/potential/issues/8) from misa-md/feature-parser-getHeader-member
- **pkg:** Merge pull request [#10](https://github.com/MISA-MD/potential/issues/10) from misa-md/bump-dependency-libs

### Style
- code formatting and typo fixing

### Test
- **eam:** add linear potential table generation fixup and eam api tests for eam/fs potential
- **eam:** add tests for `chargeDensity` and `pairPotential` of eam/alloy using linear table fixup
- **eam:** add tests for api `embedEnergy` of eam/alloy based on the linear potential table fixup
- **eam:** add linear potential table generation fixup
- **parser:** unit tests of parser api `setFilterEleTypes` for both eam/fs and eam/alloy potential
- **parser:** test eam/fs potential file parser by comparing parsed results with generated ones
- **parser:** add eam/alloy potential file parser tests: compare potential table one by one

### BREAKING CHANGE

1. Api `Parser::done`, `FuncflParser::done` and `SetflParser::done` is removed.
user should close the opened potential file manually. 2. Pass `std::istream`, instead of file path,
for class `SetflParser/FuncflParser` instantiation.


<a name="v0.3.0"></a>
## [v0.3.0] - 2023-02-28
### Chore
- **changelog:** add CHANGELOG
- **eam:** add comment for `eam::toForce` and correct comment of `eam::chargeDensity`
- **pot-loader:** add comments for member functions in classs `EamPotTableLoaderApi`

### Docs
- **changelog:** add changelog for v0.3.0
- **readme:** correct cmake option in building section of README.md

### Feat
- **eam:** add file parsing and potential calculation api for eam/fs style
- **pot-table:** add destructor for class `eam` to release memory of potential tables and splines
- **pot-table:** add support of potential table indexing for both eam/alloy and eam/fs style
- **pot-table:** replace `InterpolationLists` with `LinearArray` for storing potential tables

### Fix
- **compile:** add the missing header `<cstring>` for file `eam_base_parser.hpp` to fix compile error
- **eam:** fix the case of "rho value exceeding the table" when calculating embed energy
- **pot-table:** fix segmentation fault while syncing potential table to other MPI processes
- **pot-table:** fix index out of range in calculation caused by the incorrect size in table bcast

### Merge
- Merge pull request [#5](https://github.com/MISA-MD/potential/issues/5) from misa-md/dev
- Merge pull request [#3](https://github.com/MISA-MD/potential/issues/3) from misa-md/dev
- Merge branch 'improve-code-comments-and-style' into dev
- Merge branch 'fix-building-error-missing-header' into 'dev'
- Merge pull request [#2](https://github.com/MISA-MD/potential/issues/2) from misa-md/feature-release-potential-table-memory
- Merge pull request [#1](https://github.com/MISA-MD/potential/issues/1) from misa-md/feature-eam-fs-support
- **eam:** Merge branch 'fix-embed-energy-exceed-table' into dev
- **pot-loader:** Merge pull request [#4](https://github.com/MISA-MD/potential/issues/4) from misa-md/tests-potential-loader

### Refactor
- **atom-type:** move test file `atom_type_lists_test.cpp` to directory `tests/unit/container`
- **interpolation:** move interpolation and its list, as well as type list, into `container` dir

### Style
- **parser:** improve code style of file `setfl_parser.cpp`

### Test
- **pot-loader:** add unit tests for potential table loader for both eam/alloy and eam/fs potential

### BREAKING CHANGE

We must pass `eam_style` field to api `eam::newInstance` (currently its value can
be EAM_STYLE_ALLOY/EAM_STYLE_FS); Use loader to access potential table, instead of direct potential
table in class `eam`.


<a name="v0.2.0"></a>
## [v0.2.0] - 2021-06-18
### Build
- **cmake:** correct option `POT_TEST_BUILD_ENABLE_FLAG` usage in cmake file(missing `POT_` prefix)
- **cmake:** add `POT` prefix to cmake options

### Ci
- **gitlab-ci:** bump pkg version of docker image to 0.4.1 to fix ci building error

### Feat
- **benchmark:** add google benchmark for eam spline finding
- **eam:** make id of potential table as the order presented in potential file, instead of atomic id
- **eam:** add func dataTableSizes to obtain sizes of data in each EAM potential table
- **example:** add force calculation in demo code
- **parser:** filter element types while parsing potential file

### Fix
- **compile:** fix possible compiling error on missing header file "type_traits" for std::is_base_of

### Style
- correct comment content and format some comments style; declare var when using it.
- add file .clang-format to config project code formatting

### BREAKING CHANGE

The key parameter passed to eam api (e.g. toForce, chargeDensity, dEmbedEnergy) has
changed. Now we use the order of some material presented in potential file as the key, instead of
atomic id.


<a name="v0.1.0"></a>
## [v0.1.0] - 2020-01-22
### Build
- **cmake:** support to link gtest lib without GTest:: namespace if it is added by add_subdirectory
- **cmake:** add alias lib to make sure it can always be linked by the namespace version
- **pkg:** update googletest version from v1.8.1 to v1.10.0
- **pkg:** upgrade pkg.yaml file to version 2

### Merge
- Merge branch 'dev' into 'master' for lib pot version 0.1.0

### Refactor
- **interpolation:** inline function InterpolationObject::findSpline()

### Revert
- remove sunway code

### BREAKING CHANGE

we need pkg tool v0.4.0 or upper version to build this package


<a name="v0.1.0-beta"></a>
## [v0.1.0-beta] - 2019-05-09
### Build
- **cmake:** better cmake configuration, and correct some spell mistakes.

### Chore
- **eam:** add code comment for eam computing.

### Ci
- **gitlab-ci:** add .gitlab-ci.yml config file to build this lib.

### Docs
- **readme:** correct document mistakes in README.md file.

### Feat
- Merge branch dev into branch master for version 0.1.0-beta.
- **eam:** add feature of embedded energy and pair potential energy computing.
- **example:** add demo code for lib pot.

### Fix
- **pair:** fix pair computing(toForce) code for the different atoms types.

### Refactor
- **eam:** move spline index finding to class InterpolationObject.
- **eam:** function rename: rhoContribution -> chargeDensity, embedEnergyContribution -> dEmbedEnergy

### Style
- **eaxmple:** change code style for example/main.cpp.

### BREAKING CHANGE

function name has changed(old functions name are rhoContribution and embedEnergyContribution,
but new functions name are chargeDensity and dEmbedEnergy).

spilt param df_sum to df_from and df_to.


<a name="v0.1.0-alpha"></a>
## v0.1.0-alpha - 2019-03-11
### Build
- **cmake:** add cmake EXPORT installation for lib pot.
- **cmake:** add cmake script to build this lib.
- **pkg:** use pkg tool version 0.2.0-alpha.

### Feat
- **atom:** create a new struct AtomElement to store atom information like force,location,velocity, etc.
- **denpendency:** use newest kiwi framework (bundle put/get without MPI_Pack & MPI_Unpack).
- **eam:** add full eam adaptation for multi-atom-types.
- **eam:** using OneWayEamList to replace (InterpolationObject *rho) and (InterpolationObject *f).
- **eam:** use class EamPhiList to replace InterpolationObject array, and add better api.
- **tools:** conv tool can convert allmost binary atom file to text file.

### Fix
- **interpolation:** fix segment error in InterpolationLists::sync based on the tests in the last com
- **tools:** fixed bugs of conv tool for processing binary file (result not right).

### Merge
- Merge branch 'dev' into 'master'
- **data-structure:** Merge branch 'test-eam' into 'dev'
- **interpolation:** Merge branch 'test-interpolation-sync' into 'dev'
- **parser:** Merge branch 'refactor-eam-parser' into 'dev'

### Refactor
- rename src/config to src/types.
- remove some unnecessary code.
- **atom-prop-list:** move atom-prop-list(var type_lists) from class eam to class parser.
- **eam:** refactor eam parser, and use a elements list to store elements, and a map to store their data.
- **eam:** extract eam parser to eam_parser.cpp/.h file.
- **interpolation:** add method sync implementation in class EamPhiList and EamOneWayList to cla
- **parser:** copy parser implementation of funcfl format potential file to new interface.
- **parser:** add parser interface and move setfl format parser to class SetflParser.

### Style
- **athread_accelerate  atom crystal_md:** change code style of some files.
- **atom-prop-list:** rename menber of AtomPropsList, and removed some unnecessary code.
- **eam:** rename variables in eam.cpp/.h to underline style, and add acccess method for eam::n_
- **eam:** formated code/parameters, and removed unnecessary comments/members.
- **sunway:** move arch sunway code to directory src/arch.

### Test
- **atom-type:** add tests for class AtomPropsList.
- **data-structure:** add tests for ArrayMap.
- **interpolation:** add tests for interpolation sync in class InterpolationLists.


[Unreleased]: https://github.com/MISA-MD/potential/compare/v0.4.0...HEAD
[v0.4.0]: https://github.com/MISA-MD/potential/compare/v0.3.0...v0.4.0
[v0.3.0]: https://github.com/MISA-MD/potential/compare/v0.2.0...v0.3.0
[v0.2.0]: https://github.com/MISA-MD/potential/compare/v0.1.0...v0.2.0
[v0.1.0]: https://github.com/MISA-MD/potential/compare/v0.1.0-beta...v0.1.0
[v0.1.0-beta]: https://github.com/MISA-MD/potential/compare/v0.1.0-alpha...v0.1.0-beta
