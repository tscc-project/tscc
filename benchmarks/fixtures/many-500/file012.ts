interface Item12 { id: number; name: string; }
type Key12 = string | number;
export function make12(id: number, name: string): Item12 {
  const value: Item12 = {id, name};
  return value as Item12;
}
export const item12: Item12 = make12(12, "item-12");
