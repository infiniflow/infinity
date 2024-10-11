export type TreeParentId = string | number | null;

export interface TreeNode {
  name: string;
  id: string | number;
  children: Array<string | number>;
  parent: TreeParentId;
  isBranch?: boolean;
}
