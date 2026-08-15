interface Item472 { id: number; name: string; }
type Key472 = string | number;
export function make472(id: number, name: string): Item472 {
  const value: Item472 = {id, name};
  return value as Item472;
}
export const item472: Item472 = make472(472, "item-472");
