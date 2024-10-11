import ColumnIcon from '/public/column.svg';
import IndexIcon from '/public/index.svg';
import SegmentIcon from '/public/segment.svg';

export enum Leaf {
  Columns = 'Columns',
  Index = 'Index',
  Segments = 'Segments'
}

export const LeafIconMap = {
  [Leaf.Columns]: <ColumnIcon className="h-4 w-4"></ColumnIcon>,
  [Leaf.Index]: <IndexIcon className="h-4 w-4"></IndexIcon>,
  [Leaf.Segments]: <SegmentIcon className="h-4 w-4"></SegmentIcon>
};

export const initialData = [
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
