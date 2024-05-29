# import setuptools
#
# if __name__ == "__main__":
#     setuptools.setup(packages=['infinity', 'infinity.remote_thrift', 'infinity.remote_thrift.infinity_thrift_rpc'])

# import os
# from skbuild import setup
# from setuptools import find_packages
#
# python_package_dir = os.path.abspath("python")
# src_package_dir = os.path.abspath("src")
# print("python_package_dir:", python_package_dir)
# print("src_package_dir:", src_package_dir)
#
# setup(
#     name="emb_infinity",
#     version="0.2.0.dev2",
#     packages=find_packages(where=python_package_dir) + find_packages(where=src_package_dir),
#     package_dir={
#         "": "python",
#         "embedded_infinity": "src/embedded_infinity"
#     },
#     cmake_source_dir="src",
#     include_package_data=True,
#     zip_safe=False,
# )
#
# import os
# import subprocess
# from setuptools import setup, find_packages
# from setuptools.command.build_ext import build_ext as _build_ext
# from skbuild import setup as skbuild_setup
#
# class CustomBuildExt(_build_ext):
#     def run(self):
#         # Use scikit-build to handle the C++ build
#         skbuild_setup(
#             name="embedded_infinity",
#             version="0.2.0.dev2",
#             packages=find_packages(),
#             cmake_minimum_required_version="3.15",
#             cmake_args=[
#                 f"-DPYTHON_EXECUTABLE={os.sys.executable}"
#             ],
#             # Any additional scikit-build configuration can be added here
#         )
#         # Continue with the normal build_ext process
#         _build_ext.run(self)
#
# setup(
#     name="infinity_sdk",
#     version="0.2.0.dev2",
#     packages=find_packages(where="python"),
#     package_dir={"": "python"},
#     install_requires=[
#         "sqlglot~=11.7.1",
#         "pydantic~=2.7.1",
#         "thrift~=0.20.0",
#         "setuptools~=69.5.1",
#         "pytest~=8.2.0",
#         "pandas~=2.2.2",
#         "numpy~=1.26.4",
#         "pyarrow~=16.0.0",
#         "polars~=0.20.23",
#         "openpyxl~=3.1.2"
#     ],
#     description="infinity",
#     # long_description=open("README.md").read(),
#     # long_description_content_type="text/markdown",
#     python_requires=">=3.10",
#     cmdclass={
#         'build_ext': CustomBuildExt,
#     },
# )
# print("setup finished!")