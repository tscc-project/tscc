interface Item163 { id: number; name: string; }
type Key163 = string | number;
export function make163(id: number, name: string): Item163 {
  const value: Item163 = {id, name};
  return value as Item163;
}
export const item163: Item163 = make163(163, "item-163");
