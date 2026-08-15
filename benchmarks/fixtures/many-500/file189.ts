interface Item189 { id: number; name: string; }
type Key189 = string | number;
export function make189(id: number, name: string): Item189 {
  const value: Item189 = {id, name};
  return value as Item189;
}
export const item189: Item189 = make189(189, "item-189");
