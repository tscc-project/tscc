interface Item488 { id: number; name: string; }
type Key488 = string | number;
export function make488(id: number, name: string): Item488 {
  const value: Item488 = {id, name};
  return value as Item488;
}
export const item488: Item488 = make488(488, "item-488");
