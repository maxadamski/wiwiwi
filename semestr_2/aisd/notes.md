BST:

remove(bst, obj):
	replace obj with max(obj.left) or min(obj.right)

mktree(bst):
		median
	   /      \
	median    median
	/    \    /    \
   med  med  med  med
   ...  ...  ...  ...

wzdluzny/pre-order(bst):
	root,left,right

poprzeczny/in-order(bst):
	left,root,right

wsteczy/post-order(bst):
	left,right,root
