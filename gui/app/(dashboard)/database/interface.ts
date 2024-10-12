import { Leaf } from './constants';

export type TreeParentId = string | number | null;

export interface TreeNode {
  name: string;
  id: string | number;
  children: Array<string | number>;
  parent: TreeParentId;
  isBranch?: boolean;
}

export interface DatabaseRouteParams {
  params: { databaseId: string; tableId: string };
  searchParams: { tab: Leaf };
}
