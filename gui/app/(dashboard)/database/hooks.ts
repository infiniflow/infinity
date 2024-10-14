import { ITableColumns, ITableIndex } from '@/lib/databse-interface';
import { useRouter } from 'next/navigation';
import { useCallback, useEffect, useRef, useState } from 'react';
import { INode } from 'react-accessible-treeview';
import {
  listDatabase,
  listTable,
  showTableColumns,
  showTableIndexes,
  showTableSegments
} from '../actions';
import { initialData } from './constants';
import { DatabaseRouteParams, TreeParentId } from './interface';
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
      data: INode[];
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
  const [data, setData] = useState<INode[]>(initialData);
  const [nodesAlreadyLoaded, setNodesAlreadyLoaded] = useState<any[]>([]);
  const [loading, setLoading] = useState<boolean>(true);

  const fetchDatabases = useCallback(async () => {
    try {
      setLoading(true);
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
      setLoading(false);
    } catch (error) {
      setLoading(false);
    }
  }, []);

  const fetchTables = useCallback(async (databaseName: string) => {
    const ret = await listTable(databaseName);
    if (ret?.tables?.length > 0) {
      setData((value) => {
        const tablePropertyList: INode[] = [];
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
    } else {
      setData((value) =>
        value.map((x) => {
          let metadata = x.metadata ?? {};
          if (x.id === databaseName) {
            metadata['isEmpty'] = true;
          }
          return { ...x, metadata };
        })
      );
    }
  }, []);

  useEffect(() => {
    fetchDatabases();
  }, [fetchDatabases]);

  const onLoadData = async ({ element }: { element: INode }) => {
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

  return { wrappedOnLoadData, data, loadedAlertElement, loading };
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

export const useFetchTableIndexes = ({
  databaseId,
  tableId
}: DatabaseRouteParams['params']) => {
  const [tableIndexes, setTableIndexes] = useState<ITableIndex[]>([]);

  const fetchTableIndexes = useCallback(async () => {
    const data = await showTableIndexes({
      database_name: databaseId,
      table_name: tableId
    });

    setTableIndexes(data);
  }, []);

  useEffect(() => {
    fetchTableIndexes();
  }, [fetchTableIndexes]);

  return { tableIndexes };
};

export const useFetchTableSegments = ({
  databaseId,
  tableId
}: DatabaseRouteParams['params']) => {
  const [tableSegments, setTableSegments] = useState<any[]>([]);

  const fetchTableSegments = useCallback(async () => {
    const data = await showTableSegments({
      database_name: databaseId,
      table_name: tableId
    });

    setTableSegments(data);
  }, []);

  useEffect(() => {
    fetchTableSegments();
  }, [fetchTableSegments]);

  return { tableSegments };
};
