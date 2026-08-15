interface Item1 { id: number; name: string; }
type Key1 = string | number;
export function make1(id: number, name: string): Item1 {
  const value: Item1 = {id, name};
  return value as Item1;
}
export const item1: Item1 = make1(1, "item-1");
