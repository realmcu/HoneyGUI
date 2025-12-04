from setuptools import setup, Extension

rtk_fastlz_module = Extension(
    'rtk_fastlz',
    sources=['rtk_fastlz_module.c'],
    extra_compile_args=['/O2'] if __import__('sys').platform == 'win32' else ['-O2'],
)

setup(
    name='rtk_fastlz',
    version='1.0.0',
    description='RTK FastLz compression - compatible with Windows tool (MAX_DISTANCE=64)',
    ext_modules=[rtk_fastlz_module],
    author='HoneyGUI',
    python_requires='>=3.6',
)
