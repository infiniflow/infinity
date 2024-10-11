import { Leaf } from './constants';
import { TreeNode, TreeParentId } from './interface';

export const updateTreeData = (
  list: any[],
  id: string | number,
  children: Array<TreeNode>
) => {
  const data = list.map((node) => {
    if (node.id === id) {
      node.children = children.map((el) => {
        return el.id;
      });
    }
    return node;
  });
  return data.concat(children);
};

export const buildLeafData = (parent: string) => {
  return [
    {
      name: Leaf.Columns,
      children: [],
      id: `${Leaf.Columns}-${parent}`,
      parent
    },
    { name: Leaf.Index, children: [], id: `${Leaf.Index}-${parent}`, parent },
    {
      name: Leaf.Segments,
      children: [],
      id: `${Leaf.Segments}-${parent}`,
      parent
    }
  ];
};

export const getParentIdById = (data: TreeNode[], id: TreeParentId) => {
  return data.find((x) => x.id === id)?.parent;
};
