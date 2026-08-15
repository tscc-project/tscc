interface Item153 { id: number; name: string; }
type Key153 = string | number;
export function make153(id: number, name: string): Item153 {
  const value: Item153 = {id, name};
  return value as Item153;
}
export const item153: Item153 = make153(153, "item-153");
