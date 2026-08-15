interface Item342 { id: number; name: string; }
type Key342 = string | number;
export function make342(id: number, name: string): Item342 {
  const value: Item342 = {id, name};
  return value as Item342;
}
export const item342: Item342 = make342(342, "item-342");
