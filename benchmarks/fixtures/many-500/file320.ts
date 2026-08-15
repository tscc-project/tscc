interface Item320 { id: number; name: string; }
type Key320 = string | number;
export function make320(id: number, name: string): Item320 {
  const value: Item320 = {id, name};
  return value as Item320;
}
export const item320: Item320 = make320(320, "item-320");
