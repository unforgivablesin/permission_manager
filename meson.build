project('Socket Forwarder', 'cpp',
    default_options: ['cpp_std=c++23']
)

wx_dep = dependency('wxwidgets', required: true)
sdbus_cpp = dependency('sdbus-c++', required: true)

permission_manager_sources = [
    'service/main.cpp',
    'service/forwarder.cpp',
    'service/handler.cpp',
    'service/client.cpp',
    'service/dbus.cpp'
]

permission_dialog_sources = [
    'dialog/main.cpp',
    'dialog/dbus.cpp'
]

executable('permission_manager', permission_manager_sources, dependencies: [wx_dep, sdbus_cpp])
executable('permission_dialog', permission_dialog_sources, dependencies: [wx_dep, sdbus_cpp])
