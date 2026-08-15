interface Item67 { id: number; name: string; }
type Key67 = string | number;
export function make67(id: number, name: string): Item67 {
  const value: Item67 = {id, name};
  return value as Item67;
}
export const item67: Item67 = make67(67, "item-67");
