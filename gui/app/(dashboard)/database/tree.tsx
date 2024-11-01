'use client';

import { useHover } from 'ahooks';
import cx from 'classnames';
import TreeView, {
  EventCallback,
  INode,
  INodeRendererProps
} from 'react-accessible-treeview';
import { AiOutlineLoading } from 'react-icons/ai';
import { IoMdArrowDropright } from 'react-icons/io';
import { Leaf, LeafIconMap } from './constants';
import DatabaseIcon from '/public/database.svg';
import TableIcon from '/public/table.svg';

import { toast } from '@/components/hooks/use-toast';
import { Button } from '@/components/ui/button';
import { Spinner } from '@/components/ui/spinner';
import { DropOption } from '@/lib/constant/common';
import { MoreHorizontal } from 'lucide-react';
import { useRef } from 'react';
import { dropDatabase } from '../actions';
import { DeleteDropdown } from '../database-card';
import {
  TreeDataProps,
  useBuildTreeData,
  useHandleClickTreeName
} from './hooks';

import { PlusIcon } from '@radix-ui/react-icons';
import { DatabaseCreatingDialog } from '../database-creating-dialog';
import './styles.css';

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

const ArrowIcon = ({
  isOpen,
  className
}: {
  isOpen: boolean;
  className?: string;
}) => {
  const baseClass = 'arrow';
  const classes = cx(
    baseClass,
    { [`${baseClass}--closed`]: !isOpen },
    { [`${baseClass}--open`]: isOpen },
    className
  );
  return <IoMdArrowDropright className={classes} />;
};

const DatabaseDropdownMenu = ({
  isHovering,
  element,
  fetchDatabases
}: {
  isHovering: boolean;
  element: INode;
} & TreeDataProps) => {
  const handleDelete = async () => {
    const ret = await dropDatabase({
      database_name: element.name,
      drop_option: DropOption.IgnoreIfNotExists
    });
    if (ret.error_code === 0) {
      fetchDatabases?.();
      toast({
        title: 'Drop Success',
        description: ''
      });
    }
  };

  return (
    <DeleteDropdown deleteItem={handleDelete}>
      {/* <MoreIcon className="h-5 w-5 inline-block cursor-pointer"></MoreIcon> */}
      {isHovering && (
        <MoreHorizontal className="h-5 w-5 inline-block cursor-pointer" />
      )}
    </DeleteDropdown>
  );
};

const TreeNode = ({
  handleSelect,
  handleExpand,
  element,
  isBranch,
  level,
  getNodeProps,
  isExpanded,
  data,
  fetchDatabases
}: INodeRendererProps & { data: INode[] } & TreeDataProps) => {
  const { handleClickTreeName } = useHandleClickTreeName();
  const ref = useRef(null);
  const isHovering = useHover(ref);

  const handleNodeClick: EventCallback = (e) => {
    handleSelect(e);
    if (isBranch) {
      handleExpand(e);
    } else {
      handleClickTreeName({
        level,
        name: element.name,
        parent: element.parent,
        data
      });
    }
  };

  const branchNode = (isExpanded: boolean, element: INode) => {
    return isExpanded &&
      !element.metadata?.isEmpty &&
      element.children?.length === 0 ? (
      <>
        <span role="alert" aria-live="assertive" className="visually-hidden">
          loading {element.name}
        </span>
        <AiOutlineLoading aria-hidden={true} className="loading-icon" />
      </>
    ) : (
      <ArrowIcon isOpen={isExpanded} />
    );
  };
  const nodeProps = getNodeProps({
    onClick: handleNodeClick
  });
  nodeProps.className = nodeProps.className + 'flex justify-between pr-2';

  return (
    <div style={{ marginLeft: 40 * (level - 1) }} {...nodeProps} ref={ref}>
      <section className="flex items-center">
        {isBranch && branchNode(isExpanded, element)}
        <div className="flex items-center ml-1 ">
          {renderIcon(level, element.name)}
          <span className="name text-lg">{element.name}</span>
        </div>
      </section>
      <DatabaseDropdownMenu
        isHovering={isHovering}
        element={element}
        fetchDatabases={fetchDatabases}
      ></DatabaseDropdownMenu>
    </div>
  );
};

function AsyncTree() {
  const {
    data,
    loadedAlertElement,
    wrappedOnLoadData,
    loading,
    fetchDatabases
  } = useBuildTreeData();

  return loading ? (
    <Spinner />
  ) : (
    <div className="async-tree">
      <div
        className="visually-hidden"
        ref={loadedAlertElement}
        role="alert"
        aria-live="polite"
      ></div>
      <DatabaseCreatingDialog fetchDatabases={fetchDatabases}>
        <Button className="flex w-full mb-2 gap-2">
          <PlusIcon></PlusIcon>
          Create Database
        </Button>
      </DatabaseCreatingDialog>
      <TreeView
        data={data}
        aria-label="directory tree"
        onLoadData={wrappedOnLoadData}
        nodeRenderer={(props) => {
          return (
            <TreeNode
              {...props}
              data={data}
              fetchDatabases={fetchDatabases}
            ></TreeNode>
          );
        }}
      />
    </div>
  );
}

export default AsyncTree;
