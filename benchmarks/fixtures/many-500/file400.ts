interface Item400 { id: number; name: string; }
type Key400 = string | number;
export function make400(id: number, name: string): Item400 {
  const value: Item400 = {id, name};
  return value as Item400;
}
export const item400: Item400 = make400(400, "item-400");
