interface Item188 { id: number; name: string; }
type Key188 = string | number;
export function make188(id: number, name: string): Item188 {
  const value: Item188 = {id, name};
  return value as Item188;
}
export const item188: Item188 = make188(188, "item-188");
