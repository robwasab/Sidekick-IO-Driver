from setuptools import setup, Extension, find_packages

sidekickio_module = Extension(
	'_sidekickio',
	sources = ['sidekickio_wrap.cxx'],
	libraries = ['SidekickIOStaticLibrary', 'libusb-1.0'],
	include_dirs = [r'..\..\..'],
	library_dirs = [
		r'..\x64\Debug',
		r'..\LibUsb\MS64\static'],
)

setup(
	name = 'sidekickio',
	version = '0.1',
	author = 'Robby Tong',
	description = 'Sidekick Python Interface',
	ext_modules = [sidekickio_module],
	py_modules = ['sidekickio'],
	package_dir = {
		'': '.',
	},
)

# swig -c++ -python sidekickio.i
# python setup.py build_ext --inplace
