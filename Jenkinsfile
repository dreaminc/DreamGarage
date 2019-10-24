pipeline {
  agent any
  
  stages {

    stage('Checkout') {
      steps{
        echo 'Checking out code'
        
        checkout scm

        mkdir build
        cd build
      }
    }

    stage('Build') {
      steps {
        echo 'Building...'
        cmake --version
        cmake ..

        msbuild ALL_BUILD.vcxproj /verbosity:minimal /maxcpucount:1 /property:Configuration=%configuration%
      }
    }

    stage('Test') {
      steps {
        echo 'Testing...'
      }
    }

    stage('Deploy') {
      steps {
        echo 'Deploying...'  
      }
    }
  }
}
