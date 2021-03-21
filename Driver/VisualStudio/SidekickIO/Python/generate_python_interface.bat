swig -c++ -python sidekickio.i
python setup.py build_ext
python setup.py bdist_wheel
pip install -e .
