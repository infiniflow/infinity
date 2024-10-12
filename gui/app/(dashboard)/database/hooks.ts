import { ITableColumns } from '@/lib/databse-interface';
import { useRouter } from 'next/navigation';
import { useCallback, useEffect, useRef, useState } from 'react';
import { listDatabase, listTable, showTableColumns } from '../actions';
import { initialData } from './constants';
import { DatabaseRouteParams, TreeNode, TreeParentId } from './interface';
import { buildLeafData, getParentIdById, updateTreeData } from './utils';

export const useHandleClickTreeName = () => {
  const router = useRouter();

  const handleClickTreeName = useCallback(
    ({
      level,
      name,
      parent,
      data
    }: {
      level: number;
      name: string;
      parent: TreeParentId;
      data: TreeNode[];
    }) =>
      () => {
        if (level === 3) {
          const databaseId = getParentIdById(data, parent);
          if (databaseId) {
            router.push(`/database/${databaseId}/table/${parent}?tab=${name}`);
          }
        }
      },
    []
  );

  return { handleClickTreeName };
};

export const useBuildTreeData = () => {
  const loadedAlertElement = useRef(null);
  const [data, setData] = useState<TreeNode[]>(initialData);
  const [nodesAlreadyLoaded, setNodesAlreadyLoaded] = useState<any[]>([]);

  const fetchDatabases = useCallback(async () => {
    const ret = await listDatabase();
    if (ret.databases.length > 0) {
      setData((value) =>
        updateTreeData(
          value,
          0,
          ret.databases.map((x: string) => ({
            name: x,
            children: [],
            id: x,
            parent: 0,
            isBranch: true
          }))
        )
      );
    }
  }, []);

  const fetchTables = useCallback(async (databaseName: string) => {
    const ret = await listTable(databaseName);
    if (ret?.tables?.length > 0) {
      setData((value) => {
        const tablePropertyList: TreeNode[] = [];
        const tableList = ret.tables.map((x: string) => {
          const leafs = buildLeafData(x);
          tablePropertyList.push(...leafs);

          return {
            name: x,
            children: leafs.map((x) => x.id),
            id: x,
            parent: databaseName,
            isBranch: true
          };
        });

        return [
          ...updateTreeData(value, databaseName, tableList),
          ...tablePropertyList
        ];
      });
    }
  }, []);

  useEffect(() => {
    fetchDatabases();
  }, [fetchDatabases]);

  const onLoadData = async ({ element }: { element: TreeNode }) => {
    if (element.children.length > 0) {
      return;
    }

    await fetchTables(element.id as string);

    return undefined;
    // return new Promise((resolve) => {
    //   setTimeout(() => {
    //     setData((value) =>
    //       updateTreeData(value, element.id, [
    //         {
    //           name: `Child Node ${value.length}`,
    //           children: [],
    //           id: value.length,
    //           parent: element.id,
    //           isBranch: true
    //         },
    //         {
    //           name: 'Another child Node',
    //           children: [],
    //           id: value.length + 1,
    //           parent: element.id
    //         }
    //       ])
    //     );
    //     resolve(undefined);
    //   }, 1000);
    // });
  };

  const wrappedOnLoadData = async (props: any) => {
    const nodeHasNoChildData = props.element.children.length === 0;
    const nodeHasAlreadyBeenLoaded = nodesAlreadyLoaded.find(
      (e) => e.id === props.element.id
    );

    await onLoadData(props);

    if (nodeHasNoChildData && !nodeHasAlreadyBeenLoaded) {
      const el: any = loadedAlertElement.current;
      setNodesAlreadyLoaded([...nodesAlreadyLoaded, props.element]);
      el && (el.innerHTML = `${props.element.name} loaded`);

      // Clearing aria-live region so loaded node alerts no longer appear in DOM
      setTimeout(() => {
        el && (el.innerHTML = '');
      }, 5000);
    }
  };

  return { wrappedOnLoadData, data, loadedAlertElement };
};

export const useFetchTableColumns = ({
  databaseId,
  tableId
}: DatabaseRouteParams['params']) => {
  const [tableColumns, setTableColumns] = useState<ITableColumns[]>([]);

  const fetchTableColumns = useCallback(async () => {
    const data = await showTableColumns({
      database_name: databaseId,
      table_name: tableId
    });

    setTableColumns(data);
  }, []);

  useEffect(() => {
    fetchTableColumns();
  }, [fetchTableColumns]);

  return { tableColumns };
};
