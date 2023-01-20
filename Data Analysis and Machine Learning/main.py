#PART1 - (remove inline)
import pandas as pd #a data analysis library
import seaborn as sns #used for data visualisation
sns.set_palette('husl') #sets the colours for visualisation
import matplotlib.pyplot as plt #enables the graphs
from sklearn import metrics #measures performance
from sklearn.neighbors import KNeighborsClassifier #classifier
from sklearn.linear_model import LogisticRegression #used for accuracy
from sklearn.model_selection import train_test_split #split's data

#PART2 - get data
data = pd.read_csv('iris.csv')
input('Show data [Press ENTER]')
print(data)
input('Show head [Press ENTER]')
print(data.head())
input('Show info [Press ENTER]')
print(data.info())
input('Show description [Press ENTER]')
print(data.describe())

#PART3 - Split test and train data
X = data.drop(['Species'], axis=1)
y = data['Species']
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.4, random_state=5)

#PART4 - Try things out
k_range = list(range(1,26))
scores = []
for k in k_range:
    knn = KNeighborsClassifier(n_neighbors=k)
    knn.fit(X_train, y_train)
    y_pred = knn.predict(X_test)
    scores.append(metrics.accuracy_score(y_test, y_pred))
    
#PART5 - Show accuracy graph
plt.plot(k_range, scores)
plt.xlabel('Value of k for KNN')
plt.ylabel('Accuracy Score')
plt.title('Accuracy Scores for Values of k of k-Nearest-Neighbors')
plt.show(block=False)

#PART6 - Show accuracy
logreg = LogisticRegression()
logreg.fit(X_train, y_train)
y_pred = logreg.predict(X_test)
print("Accuracy:",metrics.accuracy_score(y_test, y_pred))

#PART7 - Built a classifier
neighboursChoice = int(input('What value of n gives best accuracy? (We think 12 works well!): '))
knn = KNeighborsClassifier(n_neighbors=neighboursChoice)
knn.fit(X, y)

SepalLengthCm = float(input('Sepal length?'))
SepalWidthCm = float(input('Sepal width?'))
PetalLengthCm = float(input('Petal length?'))
PetalWidthCm = float(input('Petal width?'))
print("Prediction:",knn.predict([[SepalLengthCm, SepalWidthCm, PetalLengthCm, PetalWidthCm]]))