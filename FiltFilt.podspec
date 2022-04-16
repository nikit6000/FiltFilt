#
# Be sure to run `pod lib lint FiltFilt.podspec' to ensure this is a
# valid spec before submitting.
#
# Any lines starting with a # are optional, but their use is encouraged
# To learn more about a Podspec see https://guides.cocoapods.org/syntax/podspec.html
#

Pod::Spec.new do |s|
  s.name             = 'FiltFilt'
  s.version          = '0.1.0'
  s.summary          = 'MatLab filtfilt function for swift'

  s.description      = <<-DESC
MatLab filtfilt function for swift
                       DESC

  s.homepage         = 'https://github.com/nikit6000/FiltFilt'
  s.license          = { :type => 'MIT', :file => 'LICENSE' }
  s.author           = { 'nikit6000' => 'tarkhov.nik@gmail.com' }
  s.source           = { :git => 'https://github.com/nikit6000/FiltFilt.git', :tag => s.version.to_s }
  s.ios.deployment_target = '9.0'

  s.source_files = ['FiltFilt/Classes/**/*.swift', 'FiltFilt/Classes/**/*.cpp', 'FiltFilt/Classes/**/*.h']
  
  s.library = 'c++'
  s.xcconfig = {
    'HEADER_SEARCH_PATHS' => '$(PODS_ROOT)/FiltFilt',
    'CLANG_CXX_LANGUAGE_STANDARD' => 'c++14',
    'CLANG_CXX_LIBRARY' => 'libc++'
  }
  s.preserve_paths = [ 'FiltFilt/Eigen/*', 'FiltFilt/Eigen/**/*']
  s.frameworks = 'Foundation'
  
end
