interface Item416 { id: number; name: string; }
type Key416 = string | number;
export function make416(id: number, name: string): Item416 {
  const value: Item416 = {id, name};
  return value as Item416;
}
export const item416: Item416 = make416(416, "item-416");
