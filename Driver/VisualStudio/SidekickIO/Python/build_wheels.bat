swig -c++ -python sidekickio.i
call activate python36
python setup.py build_ext --force
python setup.py bdist_wheel
call activate python37
python setup.py build_ext --force
python setup.py bdist_wheel
call activate python38
python setup.py build_ext --force
python setup.py bdist_wheel
call activate python39
python setup.py build_ext --force
python setup.py bdist_wheel
