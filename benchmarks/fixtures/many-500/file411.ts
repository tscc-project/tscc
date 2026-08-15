interface Item411 { id: number; name: string; }
type Key411 = string | number;
export function make411(id: number, name: string): Item411 {
  const value: Item411 = {id, name};
  return value as Item411;
}
export const item411: Item411 = make411(411, "item-411");
