typedef struct Xtal_TreeNode Xtal_TreeNode;
struct Xtal_TreeNode {
	i64 value;
	
	Xtal_TreeNode* parent;
	Xtal_TreeNode* left;
	Xtal_TreeNode* right;
};

internal Xtal_TreeNode Xtal_TreeAdd(Xtal_TreeNode* parent, u64 value) {
	Xtal_TreeNode* result = xtal_os->HeapAlloc(sizeof *result);
	result->value = value;
	if (parent != NULL) {
		result->parent = parent;
		if (value < parent->value) {
			parent->left = result;
		} else {
			parent->right = result;
		}
	}
	
}