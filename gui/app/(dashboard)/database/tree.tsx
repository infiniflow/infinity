'use client';

import cx from 'classnames';
import TreeView, { INode } from 'react-accessible-treeview';
import { AiOutlineLoading } from 'react-icons/ai';
import { IoMdArrowDropright } from 'react-icons/io';
import { Leaf, LeafIconMap } from './constants';
import DatabaseIcon from '/public/database.svg';
import TableIcon from '/public/table.svg';

import { Spinner } from '@/components/ui/spinner';
import { useBuildTreeData, useHandleClickTreeName } from './hooks';
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

function AsyncTree() {
  const { data, loadedAlertElement, wrappedOnLoadData, loading } =
    useBuildTreeData();
  const { handleClickTreeName } = useHandleClickTreeName();

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
          // isSelected,
          // isHalfSelected,
          // handleSelect,
          getNodeProps,
          level,
          handleExpand
        }) => {
          const branchNode = (isExpanded: boolean, element: INode) => {
            return isExpanded &&
              !element.metadata?.isEmpty &&
              element.children.length === 0 ? (
              <>
                <span
                  role="alert"
                  aria-live="assertive"
                  className="visually-hidden"
                >
                  loading {element.name}
                </span>
                <AiOutlineLoading aria-hidden={true} className="loading-icon" />
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
              <div
                className="flex items-center ml-1"
                onClick={handleClickTreeName({
                  level,
                  name: element.name,
                  parent: element.parent,
                  data
                })}
              >
                {renderIcon(level, element.name)}
                <span className="name">{element.name}</span>
              </div>
            </div>
          );
        }}
      />
    </div>
  );
}

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

export default AsyncTree;
