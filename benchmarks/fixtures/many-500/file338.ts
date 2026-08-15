interface Item338 { id: number; name: string; }
type Key338 = string | number;
export function make338(id: number, name: string): Item338 {
  const value: Item338 = {id, name};
  return value as Item338;
}
export const item338: Item338 = make338(338, "item-338");
