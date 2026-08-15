interface Item206 { id: number; name: string; }
type Key206 = string | number;
export function make206(id: number, name: string): Item206 {
  const value: Item206 = {id, name};
  return value as Item206;
}
export const item206: Item206 = make206(206, "item-206");
