pipeline {
  agent any
  stages {
    stage('Build') {
      steps {
        echo 'Building...'
      }
    }

    state('Test') {
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
