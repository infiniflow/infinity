'use client';

import cx from 'classnames';
import { useCallback, useEffect, useRef, useState } from 'react';
import TreeView from 'react-accessible-treeview';
import { AiOutlineLoading } from 'react-icons/ai';
import { IoMdArrowDropright } from 'react-icons/io';
import ColumnIcon from '/public/column.svg';
import DatabaseIcon from '/public/database.svg';
import IndexIcon from '/public/index.svg';
import SegmentIcon from '/public/segment.svg';
import TableIcon from '/public/table.svg';

import { listDatabase, listTable } from '../actions';
import './styles.css';

interface TreeNode {
  name: string;
  id: string | number;
  children: Array<string | number>;
  parent: string | number | null;
  isBranch?: boolean;
}

const initialData = [
  {
    name: '',
    id: 0,
    children: [1, 2, 3],
    parent: null
  },
  {
    name: 'Fruits',
    children: [],
    id: 1,
    parent: 0,
    isBranch: true
  },
  {
    name: 'Drinks',
    children: [4, 5],
    id: 2,
    parent: 0,
    isBranch: true
  },
  {
    name: 'Vegetables',
    children: [],
    id: 3,
    parent: 0,
    isBranch: true
  },
  {
    name: 'Pine colada',
    children: [],
    id: 4,
    parent: 2
  },
  {
    name: 'Water',
    children: [],
    id: 5,
    parent: 2
  }
];

enum Leaf {
  Columns = 'Columns',
  Index = 'Index',
  Segments = 'Segments'
}

const buildLeafData = (parent: string) => {
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

const LeafIconMap = {
  [Leaf.Columns]: <ColumnIcon className="h-4 w-4"></ColumnIcon>,
  [Leaf.Index]: <IndexIcon className="h-4 w-4"></IndexIcon>,
  [Leaf.Segments]: <SegmentIcon className="h-4 w-4"></SegmentIcon>
};

const renderIcon = (level: number, name: string) => {
  if (level === 1) {
    return <DatabaseIcon className="h-4 w-4"></DatabaseIcon>;
  } else if (level === 2) {
    return <TableIcon className="h-4 w-4" />;
  } else {
    return LeafIconMap[name as Leaf];
  }
  return <></>;
};

function AsyncTree() {
  const loadedAlertElement = useRef(null);
  const [data, setData] = useState<TreeNode[]>(initialData);
  const [nodesAlreadyLoaded, setNodesAlreadyLoaded] = useState<any[]>([]);

  const updateTreeData = (
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

  return (
    <>
      <div className="async-tree">
        <div
          className="visually-hidden"
          ref={loadedAlertElement}
          role="alert"
          aria-live="polite"
        ></div>
        <TreeView
          data={data}
          aria-label="Checkbox tree"
          onLoadData={wrappedOnLoadData}
          propagateSelect
          togglableSelect
          propagateSelectUpwards
          nodeRenderer={({
            element,
            isBranch,
            isExpanded,
            isSelected,
            isHalfSelected,
            getNodeProps,
            level,
            handleSelect,
            handleExpand
          }) => {
            const branchNode = (isExpanded: any, element: any) => {
              return isExpanded && element.children.length === 0 ? (
                <>
                  <span
                    role="alert"
                    aria-live="assertive"
                    className="visually-hidden"
                  >
                    loading {element.name}
                  </span>
                  <AiOutlineLoading
                    aria-hidden={true}
                    className="loading-icon"
                  />
                </>
              ) : (
                <ArrowIcon isOpen={isExpanded} />
              );
            };
            return (
              <div
                {...getNodeProps({ onClick: handleExpand })}
                style={{ marginLeft: 40 * (level - 1) }}
              >
                {isBranch && branchNode(isExpanded, element)}
                <div className="flex items-center ml-1">
                  {renderIcon(level, element.name)}
                  <span className="name">{element.name}</span>
                </div>
              </div>
            );
          }}
        />
      </div>
    </>
  );
}

const ArrowIcon = ({ isOpen, className }: any) => {
  const baseClass = 'arrow';
  const classes = cx(
    baseClass,
    { [`${baseClass}--closed`]: !isOpen },
    { [`${baseClass}--open`]: isOpen },
    className
  );
  return <IoMdArrowDropright className={classes} />;
};

export default AsyncTree;
