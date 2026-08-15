interface Item92 { id: number; name: string; }
type Key92 = string | number;
export function make92(id: number, name: string): Item92 {
  const value: Item92 = {id, name};
  return value as Item92;
}
export const item92: Item92 = make92(92, "item-92");
