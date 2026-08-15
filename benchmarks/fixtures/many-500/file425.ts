interface Item425 { id: number; name: string; }
type Key425 = string | number;
export function make425(id: number, name: string): Item425 {
  const value: Item425 = {id, name};
  return value as Item425;
}
export const item425: Item425 = make425(425, "item-425");
