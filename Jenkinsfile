pipeline {
  agent {
    dockerfile true
  }
  
  stages {
    stage('Checkout') {
      steps{
        checkout scm
        sh 'mkdir -p build local'
      }
    }

    stage('Build') {
      steps {
        echo 'Building...'
        sh '''
          cd build &&
          cmake -D CMAKE_BUILD_TYPE=Debug -D BUILD_TESTING=ON .. &&
          make
        '''
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
