from itertools import permutations
import random
import sys
import time

def getInput(file):
	points = []
	lines = [line.rstrip('\n') for line in open(file)]
	for line in lines:
		arr = line.split(' ')
		intArr = [int(i) for i in arr if i]
		points.append(intArr)
	return points


def distance(p1, p2):
    return int(round(((p1[0] - p2[0])**2 + (p1[1] - p2[1])**2) ** 0.5))


def total_distance(points):
    return (sum([distance(p, points[i + 1]) for i, p in enumerate(points[:-1])]) + distance(points[0], points[-1]))


def tsp_brute_force(points, start=None):
    if start is None:
        start = points[0]
    return min([perm for perm in permutations(points) if perm[0] == start], key=total_distance)


def tsp_approximation(points, start=None):
    if start is None:
        start = points[0]
    to_visit = points
    path = [start]
    to_visit.remove(start)
    while to_visit:
        closest = min(to_visit, key=lambda x: distance(path[-1], x))
        path.append(closest)
        to_visit.remove(closest)
    return path
	
#https://web.tuke.sk/fei-cit/butka/hop/htsp.pdf	
def tsp_two_opt(points):
	for point in range (0, len(points)):
		i = point + 1
		j = i + 1
		while j <= len(points):
			temp = swap(points, i, j)
			if total_distance(temp) < total_distance(points):
				points = temp
			j += 1
	
	return points
	
def tsp_two_opt_random(points):
	visited = [-1]
	max = 20 if len(points) > 500 else 250
	for i in range (0, max): 
		random1 = -1
		while random1 in visited:
			random1 = random.randint(0, len(points))
		visited.append(random1)	
		#random2 = random.randint(random1, len(points))
		top = len(points) if random1 + 200 > len(points) else random1+200
		newSlice = tsp_two_opt(points[random1:top])
		points[random1:random2] = newSlice
	return points
	
	
def swap(points, i, j):
	newOrder = points[:]
	newOrder[i:j] = points[j - 1: i - 1: -1]
	return newOrder
	
		
def get_path_order(pathSets, pointArrSets):
	path = []
	for i in pathSets:
		found = 0
		index = 0
		while found == 0 and index < len(pointArrSets):
			if i.issubset(pointArrSets[index]):
				found = 1
				for x in pointArrSets[index]:
					if type(x) is str:
						path.append(x)
				pointArrSets.remove(pointArrSets[index])
			index += 1
	return path
	
	
def main():
    if len(sys.argv) < 2:
        print("Invalid usage: proj4TSP.py <input-file>")
        sys.exit()
    file = sys.argv[1]
    pointArr = getInput(file)
    for p in pointArr:
        p[0] = str(p[0])
    pointArrSets = [set(p) for p in pointArr]
    points = []
    for p in pointArr:
        points.append(p[1:])     
    
    start = time.time()
    tspResults = tsp_approximation(points)
	     
    # print city ID in optimal path order
    #pathSets = [set(p) for p in tspResults]
    #pathOrder = get_path_order(pathSets, pointArrSets)
    
    if len(points) > 250:
	    two_opt = tsp_two_opt_random(tspResults)
    else:
	    two_opt = tsp_two_opt(tspResults)
	
    totalDistance = total_distance(two_opt)
    stop = time.time()
    print("Total Time: % s" % (stop-start))
    print("total Distance: %s" % totalDistance)
    pathSets = [set(p) for p in two_opt]
    pathOrder = get_path_order(pathSets, pointArrSets)
    file = file + ".tour"
    f = open(file,'w')
    f.write(str(totalDistance) + '\n')
    for p in pathOrder:
        f.write(p + '\n')
	
    #for p in pathOrder2:
        #poo.append(int(p))
	
    #print(poo)
    #poo.sort()
    #print(poo)

if __name__ == "__main__":
    main()